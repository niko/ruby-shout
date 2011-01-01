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
end

