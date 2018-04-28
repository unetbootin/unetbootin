// should be run with node.js. install dependencies by running yarn

let Octokat = require('octokat')
let {exec} = require('shelljs')
let fs = require('fs')
let getsecret = require('getsecret')

let octo = new Octokat({token: getsecret('github_api_key')})
let repo = octo.repos('unetbootin', 'unetbootin')

let version = exec('./vcs-revno').trim()

let gh = require('ghreleases')
let auth = {
  token: getsecret('github_api_key')
}

async function main() {
  let existing_releases = await repo.releases.fetch()
  for (let release_info of existing_releases.items) {
    if (release_info.tagName == version) {
      await repo.releases(release_info.id).remove()
    }
  }
  let release = await repo.releases.create({tag_name: version, body: version, name: version})
  let release_files = [
    `unetbootin-source-${version}.tar.gz`,
    `unetbootin-source-${version}.zip`,
    `unetbootin-linux-${version}.bin`,
    `unetbootin-linux64-${version}.bin`,
    `unetbootin-windows-${version}.exe`,
    `unetbootin-mac-${version}.dmg`
  ].map(x => `release/${x}`)
  await new Promise(cb => gh.uploadAssets(auth, 'unetbootin', 'unetbootin', `tags/${version}`, release_files, cb))
}

main()

