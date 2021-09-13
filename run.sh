#!/bin/bash

make && ./ray-tracer > image.ppm && open image.ppm
