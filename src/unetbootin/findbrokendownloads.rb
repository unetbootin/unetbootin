#!/usr/bin/ruby

# Supply as command line arg the path to the UNetbootin executable
# Outputs distribution:version tuples that failed to download

unetbootin = ARGV[0]

distrolist = `#{unetbootin} rootcheck=n action=listdistros`.split("\n")
distrolist.each {|distro|
  versions = `#{unetbootin} rootcheck=n distribution="#{distro}" action=listversions`.split("\n")
  versions.each {|version|
  exitstatus = `#{unetbootin} rootcheck=n method=distribution installtype=HDD distribution="#{distro}" version="#{version}" testingdownload=y autoinstall=y`
  if exitstatus.include? 'downloadfailed'
    puts distro + ':' + version
  end
  Dir.glob('/tmp/un*').each{ |filepath|
    File.delete(filepath)
  }
}

}
