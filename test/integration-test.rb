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

def remove_pkg
  command = %Q{
    cd #{base_dir}
    rm -rf pkg
  }
  `#{command}`
end

def clean_test_gem
  command = %Q{
    cd #{base_dir}
    rm -rf test/gem
  }
  `#{command}`
end

def repackage
  command = %Q{
    cd #{base_dir}
    rake repackage
  }
  `#{command}`
end

def install_gem
  command = %Q{
    cd #{base_dir}
    gem install -i test/gem/ pkg/ruby-shout-2.1.gem
  }
  `#{command}`
end

clean_gem
repackage
install_gem
remove_pkg

$LOAD_PATH.clear
$LOAD_PATH << File.join(File.dirname(__FILE__), 'gem/gems/ruby-shout-2.1')
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
    puts "sending data from #{filename}; go check http://localhost:8000/test"
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
  clean_gem
end

test_all
