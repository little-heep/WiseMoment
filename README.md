# MomentWise (Qt 5.15 Widgets)

## Build (Windows / MinGW)

From `D:\QtProject\MomentWise`:

```bash
cmake --preset qt515-mingw32
cmake --build --preset qt515-mingw32 -j
```

## Run translations (optional)

This project loads translations from `./i18n/MomentWise_zh_CN.qm` / `./i18n/MomentWise_en_US.qm`.

Generate them with Qt's `lrelease`:

```bash
"D:/download/Qt/5.15.2/mingw81_32/bin/lrelease.exe" i18n/MomentWise_zh_CN.ts -qm i18n/MomentWise_zh_CN.qm
"D:/download/Qt/5.15.2/mingw81_32/bin/lrelease.exe" i18n/MomentWise_en_US.ts -qm i18n/MomentWise_en_US.qm
```

## Hotkeys

- `Ctrl+Alt+M`: show/hide main window
- `Ctrl+Alt+P`: start/pause/resume pomodoro

