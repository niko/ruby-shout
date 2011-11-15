# -*- encoding : utf-8 -*-

require File.join(File.expand_path(File.dirname(__FILE__)), 'spec_helper')

describe "Accessors" do
  before(:each) do
    @shout = Shout.new
    @description = 'çaffé düdeldø'
    @genre = 'foobar'
    @shout.genre = @genre
    @shout.description = @description
  end
  it "should not change the original encoding" do
    pending 'rb19 only' if RUBY_VERSION < '1.9'
    @shout.description.encoding.should == @description.encoding
    @shout.genre.encoding.should == @genre.encoding
  end
  describe "#original_..." do
    it "should store the original value" do
      @shout.original_description.should == @description
      @shout.original_genre.should == @genre
    end
  end
  it "should get and set the bitrate in audio_info" do
    @shout.bitrate = "128"
    @shout.bitrate.should == "128"
  end
  describe "#initialize" do
    it "should set the properies from the opts" do
      s = Shout.new :user => 'heinz'
      s.user.should == 'heinz'
    end
  end
  describe "#charset" do
    describe "if it is set" do
      it "should use the set charset" do
        @shout.charset = 'bla'
        @shout.charset.should == 'bla'
      end
    end
    describe "if it is not set" do
      before(:each) do
        @shout.charset = nil
      end
      describe "if the format is set and MP3" do
        it "should be ISO" do
          @shout.format = Shout::MP3
          @shout.charset.should == 'ISO-8859-1'
        end
      end
      describe "if the format isn't set" do
        it "should be UTF8" do
          @shout.format = Shout::OGG
          @shout.charset.should == 'UTF-8'
        end
      end
    end
  end
end

