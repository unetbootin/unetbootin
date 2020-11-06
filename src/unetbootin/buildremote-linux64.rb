#!/usr/bin/ruby

revno = `./vcs-revno`.strip
if File.exists?("./release/unetbootin-linux-#{revno}")
	File.delete("./release/unetbootin-linux-#{revno}")
end
if !File.exists?("./release/unetbootin-source-#{revno}.zip")
	system("./build-src")
end

Dir.chdir("release")

rmexisting = <<EOR
ssh unetbootin-build-linux64 <<EOT
if [ -f unetbootin-source-#{revno}.zip ]
then
rm unetbootin-source-#{revno}.zip
fi
if [ -d unetbootin-#{revno} ]
then
rm -r unetbootin-#{revno}
fi
EOT
EOR

system(rmexisting)

upload = <<EOR
sftp unetbootin-build-linux64 <<EOT
put unetbootin-source-#{revno}.zip
EOT
EOR

system(upload)

build = <<EOR
ssh unetbootin-build-linux64 <<EOT
unzip unetbootin-source-#{revno}.zip -d unetbootin-#{revno}
cd unetbootin-#{revno}
ln -s ~/qt5-x11
./build-linux64
EOT
EOR

system(build)

download = <<EOR
sftp unetbootin-build-linux64 <<EOT
cd unetbootin-#{revno}/release
get unetbootin-linux64-#{revno}.bin
EOT
EOR

system(download)

