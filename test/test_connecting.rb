require "helper"

describe "authentication" do

  before do
    sshd
  end

  it "raises an exception if connection cannot be established" do
    assert_raises MallCop::ConnectionError do
      MallCop.connect "127.0.0.1", username, :port => 9332
    end
  end

  it "authenticates via password" do
    assert_doesnt_raise do
      MallCop.connect "127.0.0.1", username, :port => 9391, :password => password
    end
  end

end