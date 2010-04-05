module MallCop
  class Session
    ###
    # Create a new MallCop::Session bound to +socket+
    def initialize socket
      @socket = socket
    end

    def authlist_for user
      userauth_list(user).split ','
    end
  end
end
