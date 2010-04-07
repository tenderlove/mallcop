def osx?
  RUBY_PLATFORM =~ /darwin/
end

def config_file
  "#{ROOT}/.mallcop_user"
end

def config_exists?
  File.exist?(config_file)
end

def create_config(user, password)
  File.open(config_file, 'w') do |f|
    f.puts({"username" => user, "password" => password}.to_yaml)
  end
end

namespace :test do
  file "test/support/sshd/host" do
    dir = File.expand_path("../../test/support/sshd", __FILE__)
    FileUtils.mkdir_p dir
    sh "ssh-keygen -t dsa -f #{dir}/host -N ''"
  end

  if osx?
    desc "Create the test user `mallcop`"
    task :create_user do
      unless config_exists?
        password = ENV['MALLCOP_PASSWORD'] || 'omghi2u'

        # === Get an available ID
        gids = `dscl . -list /Groups PrimaryGroupID | awk '{print $2}' | sort -n`.split("\n")
        uids = `dscl . -list /Users UniqueID | awk '{print $2}' | sort -n`.split("\n")
        ids  = (gids | uids).map { |id| id.to_i }.select { |id| id >= 100 }
        id   = ((ids.first..ids.last).to_a - ids).first || ids.last + 1

        # === Create the user
        sh %'sudo dscl . create /Users/_mallcop UniqueID #{id}'
        sh %'sudo dscl . create /Users/_mallcop PrimaryGroupID #{id}'
        sh %'sudo dscl . create /Users/_mallcop NFSHomeDirectory #{ROOT}'
        sh %'sudo dscl . create /Users/_mallcop RealName "MallCop Test User 1"'
        sh %'sudo dscl . passwd /Users/_mallcop "#{password}"'
        sh %'sudo dscl . append /Users/_mallcop RecordName mallcop'

        # === Create the group
        sh %'sudo dscl . create /Groups/_mallcop'
        sh %'sudo dscl . create /Groups/_mallcop PrimaryGroupID #{id}'
        sh %'sudo dscl . append /Groups/_mallcop RecordName mallcop'
        sh %'sudo dscl . create /Groups/_mallcop RealName "MallCop Test Users"'

        create_config "mallcop", password
      end
    end

    desc "Remove the test user `mallcop`"
    task :delete_user do
      sh %'sudo dscl . delete /Users/_mallcop'
      sh %'sudo dscl . delete /Groups/_mallcop'

      rm_f config_file
    end
  end

  task :files => "test/support/sshd/host"
end