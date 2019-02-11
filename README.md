# A1000-S12-CDR-Decoder
Q.825 CDR decoder and filter program. 
Copyright (c) 2011 Aleksandr Shulaev, All Rights Reserved

Please ensure that RAD Studio C++ innstalled in your Windows, so you may build it.

This SW uses:
buffer size in bytes
for search, search numbers, which consist search pattern, not only begining with it,
           default = search numbers, which only beging with search pattern
           search transparently: first - search binary, then - parse record with found number
           default = not transparently: first - search number in each Participant Info field, then parse found record
number of subscriber A or B
number of calling subscriber A
number of called subscriber B

           
CSV output format, default = TXT output format.
Process many input files to one output file, default = one output file will create for each input file.
Determine separate folder for output files
