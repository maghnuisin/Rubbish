// Offline distortion test bench: runs a WAV through a candidate distortion model
// with Drive swept 0->10 (same ramp as the Logic P recordings), 8x oversampled,
// then matches loudness window by window to a reference recording.
// Usage: bench <model> <input.wav> <reference.wav> <out.wav> [inputGainDb]
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <functional>
#include <string>
#include <vector>

struct Audio { int rate = 48000; int channels = 1; std::vector<std::vector<double>> ch; };

static uint32_t rd32(const unsigned char* p) { return p[0] | (p[1] << 8) | (p[2] << 16) | ((uint32_t) p[3] << 24); }
static uint16_t rd16(const unsigned char* p) { return (uint16_t) (p[0] | (p[1] << 8)); }

static Audio readWav(const std::string& path)
{
    FILE* f = fopen(path.c_str(), "rb");
    if (!f) { fprintf(stderr, "cannot open %s\n", path.c_str()); exit(1); }
    std::vector<unsigned char> b;
    unsigned char buf[65536]; size_t n;
    while ((n = fread(buf, 1, sizeof buf, f)) > 0) b.insert(b.end(), buf, buf + n);
    fclose(f);
    Audio a; int bits = 0, fmt = 1; size_t pos = 12;
    while (pos + 8 <= b.size())
    {
        uint32_t size = rd32(&b[pos + 4]);
        if (!memcmp(&b[pos], "fmt ", 4)) { fmt = rd16(&b[pos + 8]); a.channels = rd16(&b[pos + 10]); a.rate = (int) rd32(&b[pos + 12]); bits = rd16(&b[pos + 22]); if (fmt == 0xFFFE) fmt = rd16(&b[pos + 32]); }
        if (!memcmp(&b[pos], "data", 4))
        {
            size_t bytes = bits / 8, frames = size / (bytes * a.channels);
            a.ch.assign(a.channels, std::vector<double>(frames));
            const unsigned char* d = &b[pos + 8];
            for (size_t i = 0; i < frames; ++i)
                for (int c = 0; c < a.channels; ++c, d += bytes)
                {
                    double v = 0;
                    if (fmt == 3 && bits == 32) { float x; memcpy(&x, d, 4); v = x; }
                    else if (bits == 16) v = (int16_t) rd16(d) / 32768.0;
                    else if (bits == 24) { int32_t x = (d[0] << 8) | (d[1] << 16) | (d[2] << 24); v = (x >> 8) / 8388608.0; }
                    else if (bits == 32) v = (int32_t) rd32(d) / 2147483648.0;
                    a.ch[c][i] = v;
                }
            return a;
        }
        pos += 8 + size + (size & 1);
    }
    fprintf(stderr, "no data in %s\n", path.c_str()); exit(1);
}

static void writeWav24(const std::string& path, const Audio& a)
{
    FILE* f = fopen(path.c_str(), "wb");
    size_t frames = a.ch[0].size(); uint32_t dataBytes = (uint32_t) (frames * a.channels * 3);
    auto w32 = [&](uint32_t v) { fwrite(&v, 4, 1, f); };
    auto w16 = [&](uint16_t v) { fwrite(&v, 2, 1, f); };
    fwrite("RIFF", 1, 4, f); w32(36 + dataBytes); fwrite("WAVEfmt ", 1, 8, f);
    w32(16); w16(1); w16((uint16_t) a.channels); w32((uint32_t) a.rate); w32((uint32_t) (a.rate * a.channels * 3)); w16((uint16_t) (a.channels * 3)); w16(24);
    fwrite("data", 1, 4, f); w32(dataBytes);
    for (size_t i = 0; i < frames; ++i)
        for (int c = 0; c < a.channels; ++c)
        {
            double v = std::fmax(-1.0, std::fmin(1.0 - 1.0 / 8388608.0, a.ch[c][i]));
            int32_t x = (int32_t) std::lround(v * 8388608.0);
            unsigned char o[3] = { (unsigned char) (x & 255), (unsigned char) ((x >> 8) & 255), (unsigned char) ((x >> 16) & 255) };
            fwrite(o, 1, 3, f);
        }
    fclose(f);
}

// ---------- 8x oversampling (Kaiser-windowed sinc, 64 taps per phase) ----------
static const int kOs = 8, kTapsPerPhase = 64, kTaps = kOs * kTapsPerPhase;

static double besselI0(double x) { double s = 1, t = 1; for (int k = 1; k < 50; ++k) { t *= (x / (2 * k)) * (x / (2 * k)); s += t; } return s; }

static std::vector<double> makeLowpass()
{
    std::vector<double> h(kTaps);
    const double cutoff = 0.45 / kOs, beta = 10.0, mid = (kTaps - 1) / 2.0;
    for (int i = 0; i < kTaps; ++i)
    {
        double t = i - mid, r = t / mid;
        double sinc = t == 0 ? 2 * cutoff : std::sin(2 * M_PI * cutoff * t) / (M_PI * t);
        h[i] = sinc * besselI0(beta * std::sqrt(std::fmax(0.0, 1 - r * r))) / besselI0(beta);
    }
    double sum = 0; for (double v : h) sum += v;
    for (double& v : h) v /= sum;
    return h;
}

static std::vector<double> upsample(const std::vector<double>& x, const std::vector<double>& h)
{
    std::vector<double> y(x.size() * kOs);
    for (size_t n = 0; n < y.size(); ++n)
    {
        double s = 0; int phase = (int) (n % kOs); long base = (long) (n / kOs);
        for (int k = 0; k < kTapsPerPhase; ++k)
        {
            long idx = base - k; if (idx < 0) break;
            s += h[phase + k * kOs] * x[(size_t) idx];
        }
        y[n] = s * kOs;
    }
    return y;
}

static std::vector<double> downsample(const std::vector<double>& x, const std::vector<double>& h, size_t outLen)
{
    std::vector<double> y(outLen);
    const size_t delay = kTaps - 1; // total delay of up+down filters in oversampled samples
    for (size_t m = 0; m < outLen; ++m)
    {
        size_t n = m * kOs + delay; double s = 0;
        for (int k = 0; k < kTaps; ++k) { if (n < (size_t) k) break; size_t idx = n - k; if (idx < x.size()) s += h[k] * x[idx]; }
        y[m] = s;
    }
    return y;
}

// ---------- models ----------
// Each model maps (input sample, drive 0..1) -> output, at the oversampled rate.
// A model may keep state, so it is created per channel.
using Model = std::function<double(double, double)>;

static double dbToGain(double db) { return std::pow(10.0, db / 20.0); }

// Sine shaper (Massive-style wavefolder) with first-order antiderivative antialiasing.
// Drive sets how far round the sine the signal is pushed: gentle saturation at the bottom,
// one fold by the middle, several folds at the top.
static Model makeSineShaper()
{
    struct State { double prevU = 0; bool first = true; };
    auto s = std::make_shared<State>();
    return [s](double x, double drive)
    {
        double gain = dbToGain(11.0 + drive * 21.0); // plugin input peak ~0.33 -> ~1.2 rad at Drive 0 (about P's 6% THD), ~12.5 rad (two folds each side) at 10
        double u = x * gain;
        double y;
        if (s->first) { y = std::sin(u); s->first = false; }
        else
        {
            double du = u - s->prevU;
            y = std::fabs(du) < 1e-6 ? std::sin(0.5 * (u + s->prevU)) : (std::cos(s->prevU) - std::cos(u)) / du;
        }
        s->prevU = u;
        return y;
    };
}

static Model makeModel(const std::string& name)
{
    if (name == "sine") return makeSineShaper();
    fprintf(stderr, "unknown model %s\n", name.c_str()); exit(1);
}

// ---------- drive ramp and loudness match ----------
static double driveAt(double seconds) { return std::fmin(1.0, std::fmax(0.0, seconds / 42.6667)); }

static std::vector<double> windowRms(const Audio& a, size_t hop, size_t win)
{
    size_t frames = a.ch[0].size(); std::vector<double> r;
    for (size_t start = 0; start < frames; start += hop)
    {
        double s = 0; size_t cnt = 0;
        for (size_t i = start >= win / 2 ? start - win / 2 : 0; i < std::min(frames, start + win / 2); ++i, ++cnt)
            for (auto& c : a.ch) s += c[i] * c[i];
        r.push_back(std::sqrt(s / std::fmax(1.0, (double) cnt * a.channels)));
    }
    return r;
}

static void matchLoudness(Audio& out, const Audio& ref)
{
    size_t hop = 480, win = 14400; // 10 ms hop, 300 ms window
    auto ro = windowRms(out, hop, win), rr = windowRms(ref, hop, win);
    size_t frames = out.ch[0].size();
    for (size_t i = 0; i < frames; ++i)
    {
        size_t w = std::min(i / hop, std::min(ro.size(), rr.size()) - 1);
        double g = ro[w] > 1e-6 ? rr[w] / ro[w] : 1.0;
        for (auto& c : out.ch) c[i] *= std::fmin(g, 30.0);
    }
}

int main(int argc, char** argv)
{
    if (argc < 5) { fprintf(stderr, "usage: bench model in.wav ref.wav out.wav [inputGainDb]\n"); return 1; }
    Audio in = readWav(argv[2]), ref = readWav(argv[3]);
    double inGain = argc > 5 ? dbToGain(atof(argv[5])) : 1.0;
    auto h = makeLowpass();
    Audio out = in;
    for (int c = 0; c < in.channels; ++c)
    {
        std::vector<double> x = in.ch[c]; for (double& v : x) v *= inGain;
        auto up = upsample(x, h);
        Model model = makeModel(argv[1]);
        double dcIn = 0, dcOut = 0, rDc = std::exp(-2 * M_PI * 10.0 / (in.rate * kOs));
        for (size_t n = 0; n < up.size(); ++n)
        {
            double y = model(up[n], driveAt(n / (double) (in.rate * kOs)));
            double hp = y - dcIn + rDc * dcOut; dcIn = y; dcOut = hp; // 10 Hz DC blocker
            up[n] = hp;
        }
        out.ch[c] = downsample(up, h, x.size());
    }
    matchLoudness(out, ref);
    writeWav24(argv[4], out);
    double peak = 0; for (auto& c : out.ch) for (double v : c) peak = std::fmax(peak, std::fabs(v));
    printf("wrote %s (peak %.2f dBFS)\n", argv[4], 20 * std::log10(peak));
}
