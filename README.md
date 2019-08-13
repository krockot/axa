Simple App-to-App Mojo IPC Demo

Clone this repo in your Chromium `src/` root (so you have `src/axa/`) and update
your Android output directory's `args.gn` to se:

```
root_extra_deps = [ "//axa" ]
```

Then build:

```
autoninja -C ${your_out_dir} axa`
```

Then run:

```
${your_out_dir}/bin/axa_client_apk install
${your_out_dir}/bin/axa_service_apk install
```

Look for the "App-to-App Demo Client" app and run it. It can display
notifications and fetch HTTP resources indirectly via Mojo IPC to the service
app.

