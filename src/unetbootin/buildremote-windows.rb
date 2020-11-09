#!/usr/bin/ruby

revno = `./vcs-revno`.strip
if File.exists?("./release/unetbootin-windows-#{revno}.exe")
	File.delete("./release/unetbootin-windows-#{revno}.exe")
end
if !File.exists?("./release/unetbootin-source-#{revno}.zip")
	system("./build-src")
end

Dir.chdir("release")

rmexisting = <<EOR
ssh unetbootin-build-windows <<EOT
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
sftp unetbootin-build-windows <<EOT
put unetbootin-source-#{revno}.zip
EOT
EOR

system(upload)

build = <<EOR
ssh unetbootin-build-windows <<EOT
unzip unetbootin-source-#{revno}.zip -d unetbootin-#{revno}
cd unetbootin-#{revno}
ln -s ~/qt5-win
./build-windows
EOT
EOR

system(build)

download = <<EOR
sftp unetbootin-build-windows <<EOT
cd unetbootin-#{revno}/release
get unetbootin-windows-#{revno}.exe
EOT
EOR

system(download)

