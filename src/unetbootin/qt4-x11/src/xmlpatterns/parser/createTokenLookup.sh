outFile="qtokenlookup.cpp"

# Watch out, the --output option is not supported in the
# gperf version that apt-get pulls in on Mac OS X.
gperf TokenLookup.gperf > $outFile
