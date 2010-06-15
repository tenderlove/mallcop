require "helper"

describe "a mallcop @session" do
  before do
    sshd
    @session = MallCop::Session.new '127.0.0.1', 9391
  end

  def start_and_authenticate
    @session.start
    @session.authenticate username, :password => password
  end

  def open_channel
    start_and_authenticate
    @session.open_channel
  end

  it "starts" do
    @session.start
  end

  it "provides a hostkey hash" do
    @session.start
    hash = @session.hostkey_hash MallCop::Session::HASH_SHA1
    assert_instance_of String, hash
  end

  it "logs in with username / password" do
    @session.start
    assert @session.authenticate username, :password => password
  end

  it "logs in fails with bad username / password" do
    @session.start
    assert !@session.authenticate('foo', :password => 'bar')
  end

  it "logs in with a public key" do
    @session.start
    public_key  = "#{sshd_support}/client.pub"
    private_key = "#{sshd_support}/client"
    assert @session.userauth_publickey_fromfile username, public_key, private_key, ''
  end

  it "opens channels" do
    start_and_authenticate
    assert_instance_of MallCop::Channel, @session.open_channel
  end

  it "requests a PTY" do
    chan = open_channel
    assert chan.request_pty("vanilla")
  end

  it "requests a shell" do
    chan = open_channel
    chan.request_pty("vanilla")
    assert chan.shell
  end
end
