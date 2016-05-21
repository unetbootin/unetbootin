#!/usr/bin/ruby

libs = `otool -L ./unetbootin.app/Contents/MacOS/unetbootin`.split("\n")
libs = libs[1..libs.length]
libs = libs.map { |x| x.split()[0] }
libs = libs.reject { |x| x.start_with?('/usr/lib') || x[0..0] == '@' }
libs.each { |x| `install_name_tool -change #{x} @executable_path/../Frameworks/#{x.sub("/usr/local/Trolltech/Qt-4.8.7/lib/", "")} ./unetbootin.app/Contents/MacOS/unetbootin` }
