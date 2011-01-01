$LOAD_PATH << File.expand_path(File.dirname(__FILE__))
require 'shout_ext'
require 'shout/string_extension_rb_18'

class Shout
  attr_accessor :charset
  
  INT_ACCESSORS    = :port, :format
  STRING_ACCESSORS = :host, :user, :username, :pass, :password, :protocol, :mount, :dumpfile,
                     :agent, :user_agent, :public, :name, :url, :genre, :description
  
  STRING_ACCESSORS.each do |accessor|
    attr_accessor :"original_#{accessor}"
    
    alias :"raw_#{accessor}"  :"#{accessor}"
    define_method accessor do
      return nil unless orig_acc = self.__send__("original_#{accessor}")
      
      decode self.__send__(:"raw_#{accessor}"), orig_acc.encoding.name
    end
    
    alias :"raw_#{accessor}=" :"#{accessor}="
    define_method :"#{accessor}=" do |value|
      self.__send__ "original_#{accessor}=", value
      
      self.__send__ :"raw_#{accessor}=", encode(value)
    end
  end
  
  alias :ext_initialize :initialize
  def initialize(opts={})
    ext_initialize
    
    self.charset = opts[:charset] || 'ISO-8859-1'
    
    accessors = STRING_ACCESSORS + INT_ACCESSORS + [:charset]
    a_opts = opts.select{ |k,v| accessors.include? k }
    
    a_opts.each{ |k,v| self.__send__ :"#{k}=", v }
  end
  
  private
    def encode(s)
      s.encode(charset, :invalid => :replace, :undef => :replace, :replace => '')
    end
    def decode(s, orig_charset)
      s.encode(orig_charset, charset, :invalid => :replace, :undef => :replace, :replace => '')
    end
end
