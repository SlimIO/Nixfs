# Developer Guide
This guide has been created for developers who want to contribute to the project.

## Prerequisites

- Node.js v10 or higher.
- Follow steps of the [Node-gyp documentation](https://github.com/nodejs/node-gyp#installation).

## Build the project
Execute these commands in order at the root of the project:

```bash
$ npm install
$ npx node-gyp configure
$ npx node-gyp build
```

## Available npm commands

| command | description |
| --- | --- |
| npm run prebuilds | Generate .node build for all operating system |
| npm run build | Configure and build the project with node-gyp |
| npm run doc | Generate JSDoc .HTML documentation (in the /docs root directory) |
| npm run coverage | Generate coverage of tests |
| npm run report | Generate .HTML report of tests coverage |

> the report command have to be triggered after the coverage command.
