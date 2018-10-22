<?php

/**
 * Add a reference sequence to the instance.
 *
 * @param $instance
 *
 * @return string
 * @throws \Exception
 */
function hardwoods_jbrowse_cmd_prepare_refseq($instance) {
  $settings = hardwoods_jbrowse_get_settings();

  $data = $settings['data_dir'];
  $bin = $settings['bin_path'];

  $path = $data . '/' . hardwoods_jbrowse_make_slug($instance->title);
  if (!file_exists($path)) {
    if (!mkdir($path)) {
      throw new Exception(
        'Unable to make data directory! Please make sure the directory 
      at ' . $data . ' exists and is writable by the current user.'
      );
    }
  }

  $out = $path . '/data';
  return hardwoods_jbrowse_safe_exec(
    'perl',
    [
      $bin . '/prepare-refseqs.pl',
      '--fasta',
      $instance->file,
      '--out',
      $out,
    ]
  );
}

/**
 * Add a track to an instance.
 *
 * @param $track
 *
 * @return string
 * @throws \Exception
 */
function hardwoods_jbrowse_cmd_add_track($track) {
  $settings = hardwoods_jbrowse_get_settings();

  $data = $settings['data_dir'];
  $bin = $settings['bin_path'];
  $instance = $track->instance;

  $path = $data . '/' . hardwoods_jbrowse_make_slug($instance->title);
  $out = $path . '/data';

  if (!file_exists($out)) {
    throw new Exception('Data directory does not exist: ' . $out);
  }

  return hardwoods_jbrowse_safe_exec(
    'perl',
    [
      $bin . '/flatfile-to-json.pl',
      '--' . $track->file_type,
      $instance->file,
      '--trackLabel',
      hardwoods_jbrowse_make_slug($track->label),
      '--key',
      $track->label,
      '--out',
      $out,
      '--trackType',
      $track->track_type,
    ]
  );
}

/**
 * Safely execute a command.
 *
 * @param string $command The path to the command to execute.
 * @param array $args Arguments passed as flag => $argument or a list of
 *   arguments as [arg1, arg2, arg3]
 * @param array $output If the output argument is present, then the
 * specified array will be filled with every line of output from the
 * command. Trailing whitespace, such as \n, is not
 * included in this array. Note that if the array already contains some
 * elements, exec will append to the end of the array.
 * If you do not want the function to append elements, call
 * unset on the array before passing it to
 * exec.
 * @param int $return_var If the return_var argument is present
 * along with the output argument, then the
 * return status of the executed command will be written to this
 *
 * @return string
 */
function hardwoods_jbrowse_safe_exec(
  $command,
  array $args = [],
  &$output = NULL,
  &$return = NULL
) {
  $cmd = escapeshellcmd($command) . ' ';
  foreach ($args as $flag => $arg) {
    if (is_string($flag)) {
      $cmd .= escapeshellarg($flag) . ' ';
    }

    $cmd .= escapeshellarg($arg) . ' ';
  }

  print "Running the following command:\n";
  print $cmd . "\n";

  return exec($cmd, $output, $return);
}
