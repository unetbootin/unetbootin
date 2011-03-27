#!/usr/bin/ruby

libs = `otool -L ./unetbootin.app/Contents/MacOS/unetbootin`.split("\n")
libs = libs[1..libs.length]
libs = libs.map { |x| x.split()[0] }
libs = libs.reject { |x| x[0..0] == '/' || x[0..0] == '@' }
libs.each { |x| `install_name_tool -change #{x} @executable_path/../Frameworks/#{x} ./unetbootin.app/Contents/MacOS/unetbootin` }
