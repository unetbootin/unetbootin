#!/usr/bin/ruby

revno = `./vcs-revno`
if Dir.exists?("release")
	`rm -r release`
end
if File.exists?("./release/unetbootin-source-#{revno}.zip")
	File.delete("./release/unetbootin-source-#{revno}.zip")
end
if File.exists?("./release/unetbootin-source-#{revno}.tar.gz")
	File.delete("./release/unetbootin-source-#{revno}.tar.gz")
end
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

