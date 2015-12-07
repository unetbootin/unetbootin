#!/bin/sh
xmllint --noout --schema ../qtokenautomaton/qtokenautomaton.xsd qxslttokenlookup.xml
java net.sf.saxon.Transform -xsl:../qtokenautomaton/qautomaton2cpp.xsl qxslttokenlookup.xml
