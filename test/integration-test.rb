# This test builds the gem (there where build errors on 1.9 without the patch),
# installs it and starts a station on mountpoint /test.
# You should have an icecast-server listening on 8000 (that's the default)
# for incoming source connections.
# Point your radio client to http://localhost:8000/test.
# Look for the metadata being correctly set ('gromozek - melange').

# test.mp3 is 'melange' by 'gromozek' http://ccmixter.org/files/gromozek/20114
# licensed CC Attribution Noncommercial (3.0) http://creativecommons.org/licenses/by-nc/3.0/

def base_dir
  File.expand_path File.join(File.dirname(__FILE__), '..')
end

def version
  File.open(File.join(base_dir, 'VERSION')).readline.strip
end

def remove_pkg
  puts '=> removing pkg/'
  command = %Q{
    cd #{base_dir}
    rm -rf pkg
  }
  `#{command}`
end

def clean_test_gem
  puts '=> removing test/test_gem_installation/'
  command = %Q{
    cd #{base_dir}
    rm -rf test/test_gem_installation
  }
  `#{command}`
end

def install_gem
  puts '=> building the gem into pkg/ and installing into test/test_gem_installation/'
  command = %Q{
    cd #{base_dir}
    rake build
    gem install --no-test --no-rdoc --no-ri --install-dir test/test_gem_installation --bindir test/test_gem_installation pkg/ruby-shout-2.1.0.gem
  }
  `#{command}`
end

clean_test_gem
install_gem
remove_pkg

$LOAD_PATH.clear
$LOAD_PATH << File.join(base_dir, "test/test_gem_installation/gems/ruby-shout-#{version}/lib")
require 'shout'

def test_all
  blocksize = 16384

  s = Shout.new
  s.host = "localhost"
  s.port = 8000
  s.mount = "/test"
  s.user = "source"
  s.pass = "hackme"
  s.format = Shout::MP3

  s.connect

  filename = File.join(File.dirname(__FILE__), 'test.mp3')

  File.open(filename) do |file|
    puts "=> sending data from #{filename}...\n     go check http://localhost:8000/test"
    m = ShoutMetadata.new
    m.add 'filename', filename
    m.add 'artist', 'gromozek'
    m.add 'title', 'melange'
    s.metadata = m

    while data = file.read(blocksize)
      s.send data
      print '.'
      s.sync
    end
  end

  s.disconnect
ensure
  clean_test_gem
end

test_all
