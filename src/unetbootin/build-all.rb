#!/usr/bin/ruby

revno = `./vcs-revno`
if File.directory?("release")
	`rm -r release`
end
Dir.mkdir("release")
system("./build-src")
macbuild = Thread.new {
	system("./buildremote-mac.rb")
}
sleep(10)
linuxbuild = Thread.new {
	system("./buildremote-linux.rb")
}
sleep(10)
winbuild = Thread.new {
	system("./build-windows")
}
winbuild.join
linuxbuild.join
macbuild.join

