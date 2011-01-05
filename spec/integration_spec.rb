# You should have an icecast-server listening on 8000 (that's the default)
# for incoming source connections.
# Point your radio client to http://localhost:8000/test.
# Look for the metadata being correctly set ('gromozek - melange').

# test.mp3 is 'melange' by 'gromozek' http://ccmixter.org/files/gromozek/20114
# licensed CC Attribution Noncommercial (3.0) http://creativecommons.org/licenses/by-nc/3.0/

require File.join(File.expand_path(File.dirname(__FILE__)), 'spec_helper')

require 'rubygems'
require 'nokogiri'
require 'open-uri'

CONNECT_DATA = {
  :host   => "localhost",
  # :host   => "192.168.178.34",
  :port   => 8000,
  :user   => "source",
  :pass   => "hackme",
  :format => Shout::MP3
}

describe "Integration Test" do
  def get_icecast_status(connection)
    Nokogiri::HTML(open("http://#{connection.host}:#{connection.port}/status.xsl"))
  end

  def get_mountpoints(connection)
    doc = get_icecast_status(connection)
    doc.css('.streamheader h3').children.empty? ? false : doc.css('.streamheader h3').children.map{|c| c.text}
  end

  def get_description(connection)
    doc = get_icecast_status(connection)
    doc.css('td.streamdata')[1].children.first.text if doc.css('td.streamdata')[1]
  end

  describe "connect and send" do
    before(:each) do
      @blocksize = 16384
      @filename = File.join(File.dirname(__FILE__), 'test.mp3')
      
      @shout = Shout.new CONNECT_DATA
      
      @meta = ShoutMetadata.new
      @meta.add 'filename', @filename
      @meta.add 'artist', 'gromozek'
      @meta.add 'title', 'melange'
    end
    after(:each) do
      @shout.disconnect
    end
    describe "blocking" do
      it "should connect and send" do
        @shout.mount = 'test_all'
        @shout.connect
        @shout.metadata = @meta

        File.open(@filename) do |file|
          loop do
            data = file.read(@blocksize)
            @shout.send data
            print "'"
            @shout.sync

            if mountpoints = get_mountpoints(@shout)
              mountpoints.should include "Mount Point /test_all"
              break
            end
          end
        end

      end
    end
    describe "non-blocking" do
      it "should connect_non_blocking and send_non_blocking" do
        pending 'rb19 only' if RUBY_VERSION < '1.9'
        @shout.mount = 'test_all_non_blocking'
        @shout.connect_non_blocking
        @shout.metadata = @meta

        File.open(@filename) do |file|
          loop do
            data = file.read(@blocksize)
            @shout.send_non_blocking data
            print "'"
            @shout.sync

            if mountpoints = get_mountpoints(@shout)
              mountpoints.should include "Mount Point /test_all_non_blocking"
              break
            end
          end
        end

      end
    end
  end

end
