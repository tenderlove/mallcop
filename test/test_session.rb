require "minitest/spec"
require "mallcop"
require 'psych'

class TestSession < MiniTest::Unit::TestCase
  describe "a mallcop session" do
    before do
      @c = Socket.new(Socket::AF_INET, Socket::SOCK_STREAM, 0)
      @c.connect(Socket.sockaddr_in(22, '127.0.0.1'))
      @config = Psych.load_file File.join ENV['HOME'], '.mallcop_test'
    end

    it "initializes" do
      session = MallCop::Session.new @c
    end

    it "starts" do
      session = MallCop::Session.new @c
      session.start
    end

    it "provides a hostkey hash" do
      session = MallCop::Session.new @c
      session.start
      hash = session.hostkey_hash MallCop::Session::HASH_SHA1
      assert_instance_of String, hash
    end

    it "returns a list of authentication schemes" do
      session = MallCop::Session.new @c
      session.start
      list = session.authlist_for @config['username']
      assert_instance_of Array, list
      assert list.include?('publickey')
    end
  end
end
