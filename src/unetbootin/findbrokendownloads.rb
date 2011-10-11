#!/usr/bin/ruby1.9

# Supply as command line arg the path to the UNetbootin executable
# Outputs distribution:version tuples that failed to download

if `whoami`.strip != 'root'
puts 'Must be run as root'
exit()
end

unetbootin = ARGV[0]

distrolist = `#{unetbootin} action=listdistros`.split("\n")
distrolist.each {|distro|
  versions = `#{unetbootin} distribution="#{distro}" action=listversions`.split("\n")
  versions.each {|version|
  exitstatus = `#{unetbootin} method=distribution installtype=HDD distribution="#{distro}" version="#{version}" testingdownload=y autoinstall=y`
  if exitstatus.include? 'downloadfailed'
    puts distro + ':' + version
  end
  Dir.glob('/tmp/un*').each{ |filepath|
    File.delete(filepath)
  }
}

}
