require "helper"

module MallCop
  describe "authentication" do

    before do
      sshd
    end

    def mallcop_connect(options = {})
      options = { :port => 9391, :password => password }.merge(options)
      MallCop.connect "127.0.0.1", username, options
    end

    it "raises an exception if connection cannot be established" do
      assert_raises(ConnectionError) do
        mallcop_connect :port => 9331
      end
    end

    it "authenticates via password" do
      assert_doesnt_raise { mallcop_connect }
    end

    it "raises the correct exception when an error occurs" do
      Session.any_instance.stubs(:native_start).returns(-1)
      Session.any_instance.stubs(:native_last_errmsg).returns("Something failed")
      assert_raises_with_msg(ConnectionError, "Something failed") do
        mallcop_connect
      end
    end

  end
end