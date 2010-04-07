require "open3"
require "erb"

module Helpers
  module SSHD

    def self.included(klass)
      klass.after do
        cleanup_sshd
      end
    end

    def root
      File.expand_path("../../../", __FILE__)
    end

    def tmp
      "#{root}/tmp/testing"
    end

    def host_file
      "#{root}/test/support/sshd/host"
    end

    def sshd_config_file
      "#{tmp}/sshd_config"
    end

    def sshd_pid_file
      "#{tmp}/sshd.pid"
    end

    def sshd_config
      template = File.read("#{root}/test/support/sshd/sshd_config.erb")
      ERB.new(template, nil, '-').result(binding)
    end

    def cleanup_sshd
      pid = sshd_pid and Process.kill 9, pid
    rescue Errno::ESRCH
      nil
    ensure
      FileUtils.rm_f(sshd_pid_file)
    end

    def setup_sshd
      FileUtils.mkdir_p(tmp)
      cleanup_sshd
      File.open(sshd_config_file, "w") { |f| f.puts sshd_config }
    end

    def sshd
      setup_sshd
      f = IO.popen "#{`which sshd`.strip} -D -h #{host_file} -f #{sshd_config_file}"
      until File.exist?(sshd_pid_file)
        sleep 0.1
      end
      f
    end

    def sshd_pid
      File.exist?(sshd_pid_file) && File.read(sshd_pid_file).to_i
    end

    def config
      @config ||= YAML.load_file "#{root}/.mallcop_user"
    end

    def username
      config['username']
    end

    def password
      config['password']
    end
  end
end