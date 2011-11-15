$LOAD_PATH << File.expand_path(File.dirname(__FILE__))
require 'shout_ext'

class Shout
  attr_writer :charset

  INT_ACCESSORS    = :port, :format
  STRING_ACCESSORS = :host, :user, :username, :pass, :password, :protocol, :mount, :dumpfile,
                     :agent, :user_agent, :public, :name, :url, :genre, :description, :bitrate

  alias :ext_initialize :initialize
  def initialize(opts={})
    ext_initialize

    (STRING_ACCESSORS + INT_ACCESSORS + [:charset]).each do |a|
      self.__send__ :"#{a}=", opts[a] if opts[a]
    end
  end

  STRING_ACCESSORS.each do |accessor|
    attr_accessor :"original_#{accessor}"

    alias :"raw_#{accessor}"  :"#{accessor}"
    define_method accessor do
      return nil unless orig_acc = self.__send__("original_#{accessor}")

      decode self.__send__(:"raw_#{accessor}"), orig_acc
    end

    alias :"raw_#{accessor}=" :"#{accessor}="
    define_method :"#{accessor}=" do |value|
      self.__send__ "original_#{accessor}=", value

      self.__send__ :"raw_#{accessor}=", encode(value)
    end
  end

  def charset
    @charset || ((format && format==Shout::MP3) ? 'ISO-8859-1' : 'UTF-8')
  end

  private
    def encode(s)
      return s unless s.is_a? String

      s.encode(charset, :invalid => :replace, :undef => :replace, :replace => '')
    end
    def decode(s, orig_string)
      return s unless s.is_a? String

      orig_charset = orig_string.encoding.name
      s.encode(orig_charset, charset, :invalid => :replace, :undef => :replace, :replace => '')
    end

end
