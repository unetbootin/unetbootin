#!/usr/bin/ruby

revno = `./vcs-revno`.strip
if File.exists?("./release/unetbootin-mac-#{revno}.zip")
	File.delete("./release/unetbootin-mac-#{revno}.zip")
end
if !File.exists?("./release/unetbootin-source-#{revno}.tar.gz")
	system("./build-src")
end

Dir.chdir("release")

rmexisting = <<EOR
ssh unetbootin-build-mac <<EOT
if [ -f unetbootin-source-#{revno}.tar.gz ]
then
rm unetbootin-source-#{revno}.tar.gz
fi
if [ -d unetbootin-#{revno} ]
then
rm -r unetbootin-#{revno}
fi
EOT
EOR

system(rmexisting)

upload = <<EOR
sftp unetbootin-build-mac <<EOT
put unetbootin-source-#{revno}.zip
EOT
EOR

system(upload)

build = <<EOR
ssh unetbootin-build-mac <<EOT
unzip unetbootin-source-#{revno}.zip -d unetbootin-#{revno}
cd unetbootin-#{revno}
./build-mac
EOT
EOR

system(build)

download = <<EOR
sftp unetbootin-build-mac <<EOT
cd unetbootin-#{revno}/release
get unetbootin-mac-#{revno}.zip
EOT
EOR

system(download)

