class Shout
  private
    def encode(s)
      s.encode(charset, :invalid => :replace, :undef => :replace, :replace => '')
    end
    def decode(s, orig_string)
      orig_charset = orig_string.encoding.name
      s.encode(orig_charset, charset, :invalid => :replace, :undef => :replace, :replace => '')
    end
end
