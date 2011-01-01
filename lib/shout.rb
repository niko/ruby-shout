$LOAD_PATH << File.expand_path(File.dirname(__FILE__))
require 'shout_ext'

class Shout
  attr_accessor :charset
  
  INT_ACCESSORS    = :port, :format
  STRING_ACCESSORS = :host, :user, :username, :pass, :password, :protocol, :mount, :dumpfile,
                     :agent, :user_agent, :public, :name, :url, :genre, :description
  
  alias :ext_initialize :initialize
  def initialize(opts={})
    ext_initialize
    
    self.charset = opts[:charset] || 'ISO-8859-1'
    
    accessors = STRING_ACCESSORS + INT_ACCESSORS + [:charset]
    a_opts = opts.select{ |k,v| accessors.include? k }
    
    a_opts.each{ |k,v| self.__send__ :"#{k}=", v }
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
  
end

case 
when RUBY_VERSION >= '1.9' then require 'shout/rb19_specific'
when RUBY_VERSION <  '1.9' then require 'shout/rb18_specific'
end