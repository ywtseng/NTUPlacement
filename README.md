# NTUlegalize
ICCAD 2017 Problem C

## How to build

```sh
make
```

Or

```sh
make release
```

## Usage

```sh
Usage:
  ./NTUlegalize [options] <tech_lef> <cell_lef> <input_def> <placement_constraints> <output_def>
Options:
  -h [ --help ]                Print help messages
  --tech_lef FILE              Technology
  --cell_lef FILE              Cell
  --input_def FILE             Global placement
  --placement_constraints FILE Placement constraints
  --output_def FILE            Legalization result
  --cpu NUM (=1)               # of CPUs
  --pgp FILE                   Plot global placement
  --plg FILE                   Plot legalization result
```
