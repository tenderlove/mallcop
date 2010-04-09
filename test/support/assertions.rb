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
  end
end