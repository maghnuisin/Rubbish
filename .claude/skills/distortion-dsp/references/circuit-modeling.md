# Circuit modelling approaches

Contents: 1. Nodal DK / nonlinear state-space · 2. Wave digital filters · 3. Approximated state-space (Macak, Cohen & Hélie) · 4. Port-Hamiltonian · 5. Neural black/grey box · 6. Moving knobs in each method · 7. Choosing for Rubbish

Primary sources read in full by the research: Holters & Zölzer DAFx-11; Chowdhury, chowdsp_wdf (arXiv 2210.12554). Others are from abstracts or summaries, flagged where details are unverified.

---

## 1. Nodal DK method (discretised K-method on nodal analysis)

- **Origin [Documented]:** Yeh, Abel & Smith, "Automated Physical Modeling of Nonlinear Audio Circuits for Real-Time Audio Effects", IEEE TASLP 2010, Part I theory, Part II BJT and tube preamps. Background in Yeh's Stanford PhD thesis (2009). [Uncertain: issue number 3 vs 4; check IEEE Xplore.]
- **Mechanics [Documented: Holters & Zölzer, DAFx-11]:**
  1. Capacitors and inductors become trapezoidal "companion" resistor + current-source pairs.
  2. Modified nodal analysis with incidence matrices builds system matrix S.
  3. Inverting S gives state-space matrices A–K.
  4. Each sample, solve the nonlinear equations (e.g. Ebers–Moll) by damped Newton, warm-started from the previous sample; it usually converged in fewer than 10 iterations and ran as a real-time LV2 plugin.
- **Tools:**
  - ACME.jl (Holters, Julia) derives the matrices from a netlist. Its docs say it is geared to computing matrices rather than running models fast, so hand-port to C++.
  - Other DK case studies: MXR Phase 90 (Eichas et al., DAFx-14); Dempwolf, Holters & Zölzer on cheaper recomputation for parametric circuits (DAFx-10).
- **Best for:** tightly coupled multi-transistor circuits with feedback.

## 2. Wave digital filters (WDF)

- **[Documented: Fettweis 1986; Chowdhury 2022]** The circuit is split into one-port elements joined by series and parallel adaptors in a tree. It is modular and each element needs only local information.
- **Limits and fixes [Documented: Werner PhD 2016; Werner et al. DAFx-15]:**
  - Classic WDFs struggle with non-series/parallel topologies and with several nonlinearities.
  - R-type adaptors (derived from modified nodal analysis) handle arbitrary topologies.
  - Multiple nonlinearities are grouped at the tree root and solved there, iteratively or with tables.
  - Bernardini & Sarti's Scattering Iterative Method and a wave-domain Newton-Raphson method (TASLP 2021) offer robust multi-nonlinearity solvers.
- **Diodes without iteration [Documented: Werner et al., JAES 2015]:** explicit Wright omega / Lambert W solution, with a fast approximation by D'Angelo et al. (DAFx-19).
- **Libraries [Documented: Chowdhury 2022]:**
  - **chowdsp_wdf:** BSD-3, header C++, compile-time templates, SIMD, R-type adaptors, deferred updates. Used in BYOD and ChowKick.
  - **RT-WDF:** run-time polymorphic, uses Armadillo.
  - **Faust wdmodels.**
- **Benchmark (seconds to process 1000 s of 48 kHz audio, 2018 Mac Mini):**

| Circuit | chowdsp_wdf compile-time | chowdsp polymorphic | wdmodels | RT-WDF |
|---|---|---|---|---|
| RC low-pass | 0.43 | 2.76 | 0.80 | 2.96 |
| Diode clipper | 2.04 | 2.09 | 7.81 | n/a |
| Bassman tone stack | 0.71 | 2.85 | 0.77 | 7.98 |

  A WDF diode clipper is therefore about 0.2% of a core before oversampling [Computed from table].
- **Tube WDF [Documented]:** Karjalainen & Pakarinen, ICASSP 2006. Koren's triode equations are the common choice "largely for historical reasons" (Aalto thesis). Koren's primary source was not retrieved [Uncertain].

## 3. State-space with approximated nonlinearity

- **[Documented: Macak & Schimmel, DAFx-10; EURASIP JASP 2011; Macak, Holters & Schimmel, DAFx-12]** Precompute or approximate the nonlinear ODE solutions (Koren triode, cubic approximations) for triode preamps, phase splitters and a Fender-type preamp.
- **[Documented: Cohen & Hélie, DAFx-10; AES 2009]** Class-A single-ended power amp and high-gain triode stage, comparing triode and power-tube models.

## 4. Port-Hamiltonian systems

- **[Documented: Falaize & Hélie, Applied Sciences 2016]** A power-balanced discretisation makes simulations passive, so stable by construction. Examples: diode clipper, BJT amp, wah. PyPHS is a related library.
- **[Uncertain]** The passivity guarantee may not hold when element values change over time.

## 5. Neural models

- **[Documented: Damskägg, Juvela, Thuillier & Välimäki, ICASSP 2019]** A WaveNet-style model of a SPICE Bassman preamp, conditioned on controls; needed pre-emphasis in the loss; 2% error at high gain.
- **[Documented: Wright, Damskägg & Välimäki, DAFx-19]** A single LSTM/GRU layer matches WaveNet at much lower CPU, as a JUCE plugin.
- **[Documented: Wright et al., Applied Sciences 2020]** About 3 minutes of training audio; listeners often couldn't tell the largest models from the amp.
- **[Documented: Parker, Esqueda & Bergner, DAFx-19]** A neural network inside a state-space structure.
- **[Documented: Steinmetz & Reiss, AES 2022]** A fast TCN conditioned on LA-2A parameters.
- **[Documented: PANAMA, arXiv 2509.26564]** Neural Amp Modeler has no knob conditioning, so changing drive means swapping models.
- **Grey-box [Documented, summary only]:** Chowdhury & Clarke's differentiable WDFs train small networks to replace circuit elements.
- **[Documented: Carson, Wright & Bilbao, DAFx25]** Neural models alias too; fine-tuning can reduce it.

## 6. Moving knobs in each method

- **DK [Documented: Holters & Zölzer 2011]:**
  - Every pot change requires new matrices.
  - Splitting S into a constant part plus a low-rank pot term and using the Woodbury identity means only a small matrix (2×2 for the Crybaby wah, instead of 15×15) is inverted per update.
  - It also removes the divide-by-zero at the pot ends.
  - The paper reports no artifact measurements.
- **WDF [Documented: Chowdhury 2022]:** a component change propagates impedance updates up to the root. R-type adaptors rebuild their scattering matrix, which is why updates are deferred until all changes are in.
- **Artifacts [Inference; no paper measured them]:**
  - Block-rate coefficient jumps with carried-over state cause stepping or small transients, worse in resonant or high-gain circuits.
  - Newton warm-starts get worse after a jump.
  - The recipe: smooth the knob value and recompute every 8–32 oversampled samples. Explicit diode solutions have no convergence issue.
- **Neural:** conditioned models take a knob as an input, so movement is smooth if training covered the range. Unconditioned models need crossfades between snapshots.
- **Key design point [Inference]:** a pre-gain "drive" into a fixed nonlinearity has **no** coefficient recomputation and so no update artifacts. That is why it suits audio-rate modulation (LFOs, Filter Follow).

## 7. Choosing for Rubbish (judgement, not a cited result)

- Rubbish is a designed multi-character saturator, not a clone of one circuit. Oversampled parametric waveshapers, optionally lightly stateful (envelope-driven bias, DC blocker), give most of the audible benefit for the least risk and CPU.
- Add a WDF stage (e.g. a diode pair with a coupling cap) only if a circuit-like, level- and frequency-dependent behaviour is specifically wanted. chowdsp_wdf would be a new third-party dependency, which CLAUDE.md says needs the owner's approval first.
- Neural modelling of Decapitator itself would mean cloning a commercial product's sound and needs conditioned training per knob. It is out of scope for prd.md and not recommended.
