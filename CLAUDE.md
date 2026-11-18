# Rubbish

## What This Is
A stereo distortion/saturation audio plugin with resonant MS-20-style filters and distortion that responds to how the sound is played. Full requirements are in `prd.md`. Build only what it specifies.

## About the Owner
- The owner has no coding or audio-development background. Explain changes in plain language, keep jargon to a minimum, and say how to hear or test each result.
- Ask before making any decision that isn't covered by `prd.md`.

## Tech Stack
- **Framework:** JUCE 9.0.5
- **Language:** C++17
- **Build system:** CMake, with CPM (CMake Package Manager) for dependencies
- **Plugin formats:** VST3 and AU only
- **Platforms:** macOS and Windows
- **JUCE modules:** `juce_dsp` (including `juce::dsp::Oversampling` for the distortion stage)

## Coding Principles
- **Single responsibility.** Every function does one thing. If you need an "and" to describe it, split it.
- **Small functions.** Over ~30 lines usually means extract sub-operations into well-named functions.
- **No god classes.** Each class represents one clear concept.
- **Composition over deep inheritance.** One or two levels at most.
- **Descriptive names over comments.** Comment why, not what.
- **Fail early and clearly.** Validate inputs at boundaries.

## Architecture Preferences
- PluginProcessor handles audio; PluginEditor handles drawing. They communicate only through AudioProcessorValueTreeState (APVTS) parameters.
- Each DSP component gets its own class: distortion characters, resonant filters, Clean Low crossover, loudness tracker, Dynamics Restore, Spread, Auto Gain, safety limiter. PluginProcessor composes them and contains no raw DSP math.
- One shared input-loudness tracker feeds both Dynamics Restore and Filter Follow.
- Define all parameters in a single function that returns the parameter layout. IDs and ranges must match `prd.md` exactly.
- Implement `getStateInformation()` / `setStateInformation()` so DAW sessions recall every setting.
- Build in stages and get each one working and audible before the next: (1) core distortion and Character, (2) filters and Filter Order, (3) output section, (4) Clean Low, Dynamics Restore, Spread, Filter Follow and Delta, (5) layout polish.

## Code Style
- JUCE conventions: camelCase for variables and methods, PascalCase for classes.
- Use the `juce::` prefix explicitly (no `using namespace juce;`).
- Prefer RAII.

## DSP Constraints
- No memory allocation, locks or blocking calls in `processBlock()`. Pre-allocate in `prepareToPlay()`.
- Use `juce::SmoothedValue` for every parameter that affects audio.
- Use `juce::dsp` processors where they fit before writing custom DSP.
- Report oversampling latency to the host.
- The safety limiter is always last and output never exceeds 0 dBFS.
- Filters must stay stable at every setting, including 100% Peak with Push on.

## Testing
- Validate builds with pluginval, and with `auval` for the AU version.
- Follow the listening-test acceptance criteria in `prd.md`.

## What NOT To Do
- Don't add features not specified in `prd.md`, and nothing from its Out of Scope list.
- Don't add third-party dependencies without asking first.
- Don't put DSP logic in the editor/UI code.
- Don't use raw owning pointers. Use `std::unique_ptr` or JUCE equivalents.
- Don't use `juce::var` or dynamic typing where static types work.
- Don't over-engineer: no deep hierarchies or unnecessary abstraction layers.
