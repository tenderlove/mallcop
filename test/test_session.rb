require "minitest/spec"
require "mallcop"
require 'psych'

class TestSession < MiniTest::Unit::TestCase
  describe "a mallcop @session" do
    before do
      @c = Socket.new(Socket::AF_INET, Socket::SOCK_STREAM, 0)
      @c.connect(Socket.sockaddr_in(22, '127.0.0.1'))
      @config = Psych.load_file File.join ENV['HOME'], '.mallcop_test'
      @session = MallCop::Session.new @c
    end

    it "starts" do
      @session.start
    end

    it "provides a hostkey hash" do
      @session.start
      hash = @session.hostkey_hash MallCop::Session::HASH_SHA1
      assert_instance_of String, hash
    end

    it "returns a list of authentication schemes" do
      @session.start
      list = @session.authlist_for @config['username']
      assert_instance_of Array, list
      assert list.include?('publickey')
    end

    ###
    # For this test to pass, make sure sshd_config has:
    #
    #  PasswordAuthentication yes
    it "logs in with username / password" do
      @session.start
      assert @session.userauth_password @config['username'], @config['password']
    end

    ###
    # For this test to pass, make sure sshd_config has:
    #
    #  PasswordAuthentication yes
    it "logs in fails with bad username / password" do
      @session.start
      assert !@session.userauth_password('foo', 'bar')
    end
  end
end
