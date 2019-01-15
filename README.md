# nn-types

This repo is intended to help provide a quick start for reverse engineering Switch
sysmodules by providing some common type definitions and FLIRT signatures.

Pull requests are welcome.

## Usage

1. Download this repository (either `git clone` or download as zip)
2. Copy sig/* to `C:\Program Files\IDA 7.0\sig\arm` (or equivalent)
3. Open IDA
4. File > Load file > Parse C header file... > nn.h
5. File > Load file > FLIRT signature file... > sdk
