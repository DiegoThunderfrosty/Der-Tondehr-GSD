# Building

The complete clean-PC procedure is documented in [README.md](README.md#clean-setup-and-build).

For a Windows computer that already has Git, Visual Studio with C++, a Windows SDK, CMake, and PowerShell:

```powershell
git clone https://github.com/DiegoThunderfrosty/Der-Tondehr-GSD.git
cd Der-Tondehr-GSD
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\setup_dependencies.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\build.ps1
```

Expected output:

```text
build\windows\out\DerTondehrGSD.vst3\
```

To use an existing iPlug2 checkout:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\build.ps1 -IPlug2Dir "C:\path\to\iPlug2"
```

To select another generated build directory:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\build.ps1 -BuildDirectory build\another-directory
```

The compatible framework revision is pinned in `scripts/setup_dependencies.ps1`. Do not update it without rebuilding the Release VST3 and validating lifecycle, automation, bypass, every oversampling mode, and representative sample rates.
