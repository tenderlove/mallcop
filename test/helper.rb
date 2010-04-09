require "minitest/spec"
require "fileutils"
require "yaml"
require "mallcop"
require "mocha"

Dir["#{File.expand_path("../support", __FILE__)}/*.rb"].each { |f| require f }

MiniTest::Unit.autorun

module MiniTest
  Unit.autorun

  class Spec
    include Helpers::SSHD
    include Helpers::Assertions
  end
end