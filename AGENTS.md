<!-- SPDX-FileCopyrightText: 2026 Roman Koch -->
<!-- SPDX-License-Identifier: MIT -->

# Arbeitsregeln

Sprich mit Roman auf Deutsch.

## Entwicklung und Integration

- Beginne Featurebranches auf dem aktuellen `develop`. `develop` ist der Integrationsbranch. Übernimm abgeschlossene Featurebranches mit `git merge --ff-only`. Übernimm freigegebene Stände aus `develop` ebenfalls per Fast-Forward nach `main`.
- Entwickle Verhaltensänderungen nach Red–Green–Refactor. Reproduziere bei einem Fehler zuerst das Problem mit einem fehlschlagenden Test. Bei Build- und Paketierungsänderungen prüfe den betroffenen Build- oder Installationspfad vor und nach der Änderung.
- Prüfe die betroffenen Fälle lokal vor der Integration. Gib bei der Übergabe an, was auf dem Host, im Crossbuild und am echten Gerät geprüft wurde.
- Jede neue oder angepasste Datei erhält eine SPDX-Kopfzeile in ihrer Kommentarsyntax. Bewahre die tatsächlichen Urheber und die Lizenz von Fremdcode.
- Halte die Keypad-Repositories eigenständig. Geschützte PQ-Quellen und Zugangsdaten bleiben außerhalb dieser Repositories und ihrer CI.

Lege ausführbare Projektskripte unter `.scripts/` und eingebundene CMake-Module unter `.cmake/` ab.

## Firmware

Dieses Repository baut die RP2040-Firmware. Für den 15SBLA-Prototyp gehören `VARIKEY_15SBLA_PROTO` und `KEYPAD_15SBLA` zusammen. Andere unterstützte Varianten müssen weiterhin baubar bleiben.

```sh
cmake -S . -B build/15sbla -DCMAKE_BUILD_TYPE=Release \
  -DHARDWARE_PLATFORM=VARIKEY_15SBLA_PROTO -DFIRMWARE_PRODUCT=KEYPAD_15SBLA
cmake --build build/15sbla
```

Der automatische Download verwendet die in CMake gebundene Pico-SDK-Revision. Bei einem expliziten lokalen `PICO_SDK_PATH` gib im Buildnachweis auch dessen Revision an. Prüfe ELF und UF2 der ausgewählten Variante. Flashen ist ein separater Arbeitsschritt am Gerät.
