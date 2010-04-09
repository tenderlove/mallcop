module Helpers
  module Assertions
    def assert_doesnt_raise msg = nil
      raised = false
      begin
        yield
      rescue Exception => e
        msg = "Expected nothing to be raised, but got #{e.class} (#{e.message})"
        raised = true
      end

      assert !raised, msg
    end

    def assert_raises_with_msg err, msg
      raised = false
      begin
        yield
      rescue Exception => e
        raised = true
        assert e.is_a?(err), "Expected #{err}, but got #{e.class}"
        assert e.message == msg, "Expected #{err} (#{msg}), but got message '#{e.message}'"
      end
      assert raised, "Expected #{err} (#{msg}), but nothing was raised"
    end
  end
end