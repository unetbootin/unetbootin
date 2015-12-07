#!/bin/sh
# Generate ReportContext.h by invoking createReportContext.xsl, which
# in turns performs a transformation on the specs, fetched from http://www.w3.org/.

target="qreportcontext_p.h"
tmpFile=".reportContextInput.tmp.xml"
echo "<dummy/>" > $tmpFile
p4 revert $target
p4 edit $target
xsltproc createReportContext.xsl $tmpFile > $target
p4 revert -a $target
rm -f $tmpFile
