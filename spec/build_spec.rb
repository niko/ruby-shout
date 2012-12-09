require 'rspec'

BASE_DIR = File.expand_path File.join(File.dirname(__FILE__), '..')
VERSION = File.open(File.join(BASE_DIR, 'VERSION')).readline.strip

describe "Build process" do
  def remove_pkg
    command = %Q{
      cd #{BASE_DIR}
      puts "rm -rf pkg in #{BASE_DIR}"
      rm -rf pkg
    }
    c = `#{command}`
    puts c
    return c
  end

  def clean_test_gem
    command = %Q{
      cd #{BASE_DIR}
      echo "rm -rf spec/test_gem_installation in #{BASE_DIR}"
      rm -rf spec/test_gem_installation
    }
    c = `#{command}`
    puts c
    return c
  end

  def install_gem
    command = %Q{
      cd #{BASE_DIR}
      rake build
      gem install --no-rdoc --no-ri --install-dir spec/test_gem_installation --bindir spec/test_gem_installation pkg/ruby-shout-#{VERSION}.gem
    }
    c = `#{command}`
    puts c
    return c
  end

  it "should build" do
    clean_test_gem.should be_true
    install_gem.should be_true
    remove_pkg.should be_true
  end

end
