# Sources

Gathered 2026-11-27. "Read" = full text read by the research. "Abstract/snippet" = only an abstract, listing or search summary was seen, so details may be unverified. Summaries are in our own words.

## Waveshaping, harmonics, stateful distortion

- **Pakarinen & Yeh (2009)**, "A Review of Digital Techniques for Modeling Vacuum-Tube Guitar Amplifiers", *Computer Music Journal* 33(2):85–100. *Read.*
  - The key survey: tube nonlinearity sources (bias shift, grid current, sag, transformer hysteresis), static vs dynamic models, and patents.
  - https://www.effectrode.com/wp-content/uploads/2018/08/a_review_of_digital_techniques_for_modeling_guitar_amplifiers.pdf
- **Chowdhury (2019)**, "Real-Time Physical Modelling for Analog Tape Machines", DAFx-19. *Read.*
  - Jiles–Atherton hysteresis with bias, RK4 solver, 16x oversampling.
  - https://dafx.de/paper-archive/2019/DAFx2019_paper_3.pdf
- **Aiken**, "What Is 'Blocking' Distortion?". *Read.*
  - Grid conduction, coupling-cap charging, clamping and recovery.
  - https://aikenamps.com/index.php/what-is-blocking-distortion
- **J. O. Smith**, "DC Blocker", *Introduction to Digital Filters*, CCRMA. *Read.*
  - The one-pole/one-zero DC blocker and the choice of R.
  - https://ccrma.stanford.edu/~jos/filters/DC_Blocker.html
- **J. D. Cook (2020)**, "Clipped sine waves". *Read.*
  - Fourier series of a clipped sine; non-monotonic 5th harmonic.
  - https://www.johndcook.com/blog/2020/05/20/clipped-sine-waves/
- **Vallejo Reyes (2024)**, "Psychoacoustics of Soft Clipping", AES Latin American Conv. *Read.*
  - Perceived harshness vs clipping steepness.
  - https://eprints.whiterose.ac.uk/id/eprint/217474/
- **Hayes et al. (2023)**, DDSP review. *Snippet.*
  - Waveshaping synthesis and Chebyshev polynomials; normalisation after shaping.
  - https://arxiv.org/pdf/2308.15422
- **Puckette**, *Theory and Techniques of Electronic Music*, waveshaping chapter. *Snippet* (page fetch failed).
  - https://msp.ucsd.edu/techniques/latest/book-html/node84.html
- **Le Brun (1979)**, "Digital Waveshaping Synthesis", JAES 27(4). *Not read* (cited via others).
- **Fourier decay course notes** (UC Davis, Marquette); Nissilä, arXiv 1805.02445. *Read/snippet.*
  - Smoothness ↔ coefficient decay.
  - https://www.math.ucdavis.edu/~saito/courses/129.s10/hw4sol.pdf · https://www.mscsnet.mu.edu/~ahmed/Extra_PDEs_coeffs.pdf
- **Yeh (2009)**, PhD thesis, Stanford. *Located, not read.*
  - https://ccrma.stanford.edu/~dtyeh/papers/DavidYehThesisdoublesided.pdf

## Aliasing and antialiasing

- **Parker, Zavalishin & Le Bivic (2016)**, "Reducing the Aliasing of Nonlinear Waveshaping Using Continuous-Time Convolution", DAFx-16. *Abstract.*
  - The origin of first-order ADAA.
  - https://dafx.de/paper-archive/details/vem_XXF5qBbfiWOH2RVVAA
- **Bilbao, Esqueda, Parker & Välimäki (2017)**, "Antiderivative Antialiasing for Memoryless Nonlinearities", IEEE SPL 24(7). *Abstract.*
  - Higher-order ADAA.
  - https://aaltodoc.aalto.fi/items/470aab15-1702-4ccf-a148-24e6173079fb
- **Holters (2019/2020)**, "Antiderivative Antialiasing for Stateful Systems", DAFx-19 / Applied Sciences 10(1):20. *Read.*
  - ADAA's half-sample delay and low-pass; extension to feedback systems.
  - https://www.hsu-hh.de/ant/wp-content/uploads/sites/699/2020/10/DAFx2019_paper_4.pdf
- **Esqueda, Bilbao & Välimäki (2016)**, "Aliasing Reduction in Clipped Signals", IEEE TSP 64(20). *Read.*
  - polyBLAMP; +12/+20 dB SNR; oversampling comparison.
  - https://www.pure.ed.ac.uk/ws/files/26997332/07499828.pdf
- **Esqueda, Välimäki & Bilbao (2016)**, "Rounding Corners with BLAMP", DAFx-16. *Title.*
  - https://dafx.de/paper-archive/details/xRM0GiuF-JmeN0_aaP9VNg
- **Vicanek (2023/24)**, "Note on Alias Suppression in Digital Distortion". *Read.*
  - Oversampling vs a 1/f spectrum; ADAA kernel analysis; stable x/√(1+x²); ADAA + 4x recommendation.
  - https://www.vicanek.de/articles/AADistortion.pdf
- **Chowdhury**, "Antiderivative Anti-Aliasing" notebook, CCRMA. *Read.*
  - Practical ADAA code, ill-conditioning fallback, hard-clip examples.
  - https://ccrma.stanford.edu/~jatin/Notebooks/adaa.html
- **JUCE**, `juce_Oversampling.cpp` (master) and docs. *Read.*
  - Filter specs, latency maths, integer-latency option.
  - https://raw.githubusercontent.com/juce-framework/JUCE/master/modules/juce_dsp/processors/juce_Oversampling.cpp · https://docs.juce.com/master/classdsp_1_1Oversampling.html
- **JUCE forum**, "New Oversampling class" (author IvanC). *Read.*
  - The IIR is not truly minimum phase; latency reporting.
  - https://forum.juce.com/t/dsp-module-discussion-new-oversampling-class/24153
- **Carson, Wright & Bilbao (2025)**, "Anti-aliasing of neural distortion effects via model fine tuning", DAFx25. *Abstract.*
  - https://arxiv.org/abs/2505.11375v1
- **Lehtonen, Pekonen & Välimäki (2012)**, audibility of aliasing in sawtooth signals, JASA. *Abstract.*
  - Masking; A-weighted noise-to-mask ratio.
- **Schimmel (2012)**, Radioengineering 21(1). *Abstract.*
  - Masking of aliases.
  - https://www.radioeng.cz/fulltexts/2012/12_01_0056_0062.pdf
- **Fischmann (2023)**, "When Should You Use Oversampling?", *Sound On Sound*. *Read.*
  - Practical side effects; aliasing as character; audibility.
  - https://www.soundonsound.com/techniques/when-should-you-use-oversampling
- **Follow-ups, titles only:**
  - La Pastina & D'Angelo, ADAA with frequency compensation: https://dafx.de/paper-archive/details/vhT2Sz-JpSJ4Br8g236cRg
  - Holters 2022, combined derivative/antiderivative: https://www.dafx.de/paper-archive/details/ipdAiRl7vCBKH-p3HY6_IQ
  - Zheleznov & Bilbao DAFx24: https://dafx.de/paper-archive/2024/papers/DAFx24_paper_33.pdf
  - Albertini, Bernardini & Sarti DAFx-20, ADAA in WDFs: https://re.public.polimi.it/retrieve/e0c31c10-6d11-4599-e053-1705fe0aef77/DAFx2020_paper_35.pdf

## Parameter changes, smoothing, time-varying filters

- **JUCE docs**, `SmoothedValue` and `ValueSmoothingTypes`. *Read.*
  - https://docs.juce.com/master/classjuce_1_1SmoothedValue.html
- **Zavalishin (2018)**, *The Art of VA Filter Design* 2.1.0; "Preserving the LTI system topology". *Located; §7.12 not read.*
  - TPT/ZDF filters behave well under modulation.
  - https://www.native-instruments.com/fileadmin/ni_media/downloads/pdf/VAFilterDesign_2.1.0.pdf
- **Laroche (2007)**, "On the Stability of Time-Varying Recursive Filters", JAES 55(6). *Abstract (paywalled).*
  - https://www.aes.org/e-lib/browse.cfm?elib=14168
- **Werner & McClellan (2022)**, "Time-Varying Filter Stability and State Matrix Products", DAFx. *Abstract.*
  - https://www.dafx.de/paper-archive/details/RdjkWSf80p3vUeP3_STX_w
- **Wishnick (2014)**, "Time-Varying Filters for Musical Applications", DAFx-14. *Abstract.*
  - The SVF is recommended and proven stable when time-varying.
  - https://dafx.de/paper-archive/details/yfd4LbdXr2Cywsqxfby8ug
- **Steinberg VST3 dev portal**, sample-accurate parameter queues. *Read.*
  - https://steinbergmedia.github.io/vst3_dev_portal/pages/Tutorials/Advanced+VST+3+techniques.html
- **Apple**, `AudioUnitParameterEvent` (ramped events); Logic Pro manual on sample-accurate automation. *Read.*
  - https://developer.apple.com/tutorials/data/documentation/audiotoolbox/audiounitparameterevent/eventvalues.md · https://help.apple.com/logicpro/mac/9.1.6/en/logicpro/usermanual/chapter_28_section_0.html
- **JUCE forum threads** (anecdotal):
  - How Logic delivers automation: https://forum.juce.com/t/how-does-sample-accurate-host-plugin-automation-work/2117 · https://forum.juce.com/t/glitches-with-logic-pro-x-automations/29172 · https://forum.juce.com/t/getting-real-i-o-buffer-size-when-using-logic-pro/22604
  - Dry/wet latency: https://forum.juce.com/t/dry-wet-mixer-latency/47031
- **Adriaensen and Goetze**, linux-audio-dev 2013. *Read.*
  - Smoothing vs crossfading filters; state-copy spikes.
  - https://lists.linuxaudio.org/hyperkitty/list/linux-audio-dev@lists.linuxaudio.org/message/QGVMNE6RGC3BNBYZE465SDDXY3PM6W2H
- **SMC 2024**, paper id118. *Read via summary.*
  - Crossfading delay lines causes comb filtering.
  - https://smcnetwork.org/smc2024/papers/SMC2024_paper_id118.pdf

## Circuit modelling

- **Yeh, Abel & Smith (2010)**, "Automated Physical Modeling of Nonlinear Audio Circuits…", IEEE TASLP (Part I/II; issue number conflicting). *Abstract.*
  - https://ccrma.stanford.edu/~dtyeh/papers/yeh10_taslp.pdf
- **Holters & Zölzer (2011)**, wah-wah pedal with the nodal DK method and variable parts, DAFx-11. *Read.*
  - Woodbury low-rank pot updates; Newton details.
  - https://www.hsu-hh.de/ant/wp-content/uploads/sites/699/2017/10/Holters-Zölzer-2011-Physical-Modelling-of-a-Wah-wah-Effect-Pedal-as-a-Case-Study-for-Application-of-the-Nodal-DK-Method-to-Circuit.pdf
- **ACME.jl** (Holters). *Docs read.*
  - https://www.github.com/HSU-ANT/ACME.jl
- **Chowdhury (2022)**, "chowdsp_wdf: An Advanced C++ Library for Wave Digital Circuit Modelling". *Read.*
  - https://arxiv.org/pdf/2210.12554 · https://github.com/Chowdhury-DSP/chowdsp_wdf
- **Werner (2016)**, PhD thesis, Stanford. *Abstract.*
  - https://purl.stanford.edu/jy057cz8322
- **Werner, Nangia, Smith & Abel (2015)**, WDFs with multiple/multiport nonlinearities, DAFx-15. *Abstract.*
  - https://dafx.de/paper-archive/details/4fsq4pZNdCbUzyzAL8xsQg
- **Werner et al. (2015)**, improved diode clipper model for WDFs, JAES. *Cited via Chowdhury 2022.*
- **Bernardini et al. (2021)**, wave-digital Newton–Raphson, IEEE/ACM TASLP. *Abstract.*
  - https://re.public.polimi.it/bitstream/11311/1203185/1/WD_NR_TASLP_Bernardini_etal_2021_RG.pdf
- **Macak & Schimmel (2010)**, DAFx-10. *Abstract.*
  - https://dafx.de/paper-archive/details/KotNLLBUHr-Wb7CEhWR_DA
- **Cohen & Hélie (2010)**, DAFx-10. *Abstract.*
  - https://dafx.de/paper-archive/details/yZNxN3FPA2HJUN5vKmCG0A
- **Falaize & Hélie (2016)**, port-Hamiltonian circuits, Applied Sciences 6(10):273. *Abstract.*
  - https://mdpi.com/2076-3417/6/10/273
- **Damskägg, Juvela, Thuillier & Välimäki (2019)**, "Deep Learning for Tube Amplifier Emulation", ICASSP. *Abstract.*
  - https://arxiv.org/pdf/1811.00334
- **Wright, Damskägg & Välimäki (2019)**, real-time black-box RNN modelling, DAFx-19. *Abstract.*
  - https://acris.aalto.fi/ws/portalfiles/portal/36768418/ELEC_Wright_Real_time_black_box_DAFx2019.pdf
- **Wright, Damskägg, Juvela & Välimäki (2020)**, real-time guitar amp emulation, Applied Sciences 10(3):766. *Abstract.*
  - https://acris.aalto.fi/ws/portalfiles/portal/41964332/Real_time_guitar_amplifier_emulation.pdf
- **Parker, Esqueda & Bergner (2019)**, nonlinear state-space modelling with a deep neural network, DAFx-19. *Abstract.*
  - https://dafx.de/paper-archive/2019/DAFx2019_paper_42.pdf
- **Steinmetz & Reiss (2022)**, efficient TCN compressor modelling, AES 152. *Abstract.*
  - https://arxiv.org/abs/2102.06200

## Commercial saturators

- **Soundtoys**, Decapitator User's Guide v5 (2015; third-party copy) and product page. *Read.*
  - https://manualzilla.com/doc/5871926/soundtoys-decapitator-manual · https://www.soundtoys.com/product/decapitator/
- **Lambert (2010)**, "Plug-in Modelling: How Industry Experts Do It", *Sound On Sound*. *Read.*
  - Soundtoys, UAD, Softube and McDSP approaches.
  - https://www.soundonsound.com/techniques/plug-in-modelling-how-industry-experts-do-it
- **FabFilter**, Saturn 2 help pages. *Read / snippet for the HQ figures.*
  - https://www.fabfilter.com/help/saturn/using/bandcontrols · https://fabfilter.com/help/saturn/using/inputoutput
- **Klanghelm**, IVGI2 manual. *Search summary only.*
  - https://Klanghelm.com/docs/IVGI2-manual.pdf
- **Kazrog**, KClip 3 guide; KClip Zero page. *Read.*
  - https://m.barryrudolph.com/recall/manuals/kazrog_kclip_3.pdf · https://kazrog.com/collections/dynamics/products/kclip-zero
- **Sonnox**, Oxford Inflator manual. *Read.*
  - https://media.uaudio.com/support/manuals/dd/Oxford%20Inflator%20Manual.pdf
- **Cycling '74**, `abl.dsp.saturator~` reference (Ableton Saturator curves). *Read.*
  - https://docs.cycling74.com/reference/abl.dsp.saturator~
- **Airwindows**, Density page; oversampling opinion. *Read / snippet.*
  - https://www.airwindows.com/density/ · https://airwindows.com/antialiasing-plugins
- **KVR**, "Decapitator: Is it really the bees knees?" (user aliasing reports, low reliability).
  - https://www.kvraudio.com/forum/viewtopic.php?p=8520723

## Measurement

- **Farina (2000)**, "Simultaneous measurement of impulse response and distortion with a swept-sine technique", AES 108th. *Abstract.*
  - https://aes2.org/publications/elibrary-page/?id=10211
- **Novak et al. (2015)**, "Synchronized Swept-Sine: Theory, Application, and Implementation", JAES. *Abstract.*
  - https://www.aes.org/e-lib/browse.cfm?elib=18042
- **Novak et al. (2010)**, nonlinear system identification with swept sines; overdrive pedals (EURASIP). *Abstract.*
  - https://link.springer.com/article/10.1155/2010/793816
- **REW help**, distortion graph and multitone. *Read.*
  - https://www.roomeqwizard.com/help/help/html/graph_distortion.html
- **RaneNote 145**; Audio Precision, "More about IMD". *Read.*
  - IMD standards.
  - https://www.ranecommercial.com/legacy/note145.html · https://www.ap.com/news/more-about-imd
- **Tan, Moore & Zacharov (2003)**, JAES 51(11). *Abstract.*
  - A multitone-based perceptual distortion measure.
  - https://www.aes.org/e-lib/browse.cfm?elib=12465
- **Geddes & Lee (2003)**, AES. *Abstract.*
  - THD/IMD don't predict ratings; the GedLee metric does.
  - https://aes2.org/publications/elibrary-page/?id=12197
- **DDMF**, Plugin Doctor manual. *Snippet only.*
  - https://www.ddmf.eu/pdfmanuals/PlugindoctorManual.pdf

## Project's own computations (not published)

- Tail slopes, biased-tanh harmonic table, hard-clip null drives: research notes, 2026-11-27.
- Null-vs-drive table for hard, tanh, h=1.6, h=2.3 and h=8 curves with and without bias: computed in this session with a 4096-point single-cycle DFT.

Full research notes with more detail: the shared session-notes folder `research_notes/Distortion DSP expert knowledge/` (outside the repo).
