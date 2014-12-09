#!/bin/bash

pathToModelDir=$1

octave getModelInfo $pathToModelDir
./createMd2 $pathToModelDir
rm $pathToModelDir/*.in
