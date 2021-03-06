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
  hardwoods_jbrowse_safe_exec(
    'perl',
    [
      $bin . '/prepare-refseqs.pl',
      '--fasta',
      $instance->file,
      '--out',
      $out,
    ],
    $ignore,
    $ret
  );

  return $ret;
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
  $menu_template = $settings['menu_template'];

  $path = $data . '/' . hardwoods_jbrowse_make_slug($instance->title);
  $out = $path . '/data';

  if (!file_exists($out)) {
    throw new Exception('Data directory does not exist: ' . $out);
  }

  switch ($track->track_type)
  {
    case 'HTMLVariants':
      $instance = hardwoods_jbrowse_get_instance($track->instance_id);
      $json = hardwoods_jbrowse_get_json($instance);
      $directory = 'vcf';

      $file_name = $track->file;
      if (is_dir($track->file)) {
        $file_name = glob($track->file . '/' . '*.vcf.gz')[0];
      }
      $file_name = pathinfo($file_name)['basename'];

      $json['tracks'][] = [
        'label' => $track->label,
        'key' => $track->label,
        'storeClass' => 'JBrowse/Store/SeqFeature/VCFTabix',
        'urlTemplate' => $directory . '/' . $file_name,
        'type' => $track->track_type,
      ];

      hardwoods_jbrowse_save_json($instance, $json);
      break;

    case 'XYPlot':
      $instance = hardwoods_jbrowse_get_instance($track->instance_id);
      $json = hardwoods_jbrowse_get_json($instance);
      $basename = pathinfo($track->file)['basename'];

      $json['tracks'][] = [
        'label' => $track->label,
        'key' => $track->label,
        'storeClass' => 'JBrowse/Store/SeqFeature/BigWig',
        'urlTemplate' => 'wig/' . $basename,
        'type' => 'JBrowse/View/Track/Wiggle/XYPlot',
      ];

      hardwoods_jbrowse_save_json($instance, $json);
      break;

    default:
    hardwoods_jbrowse_safe_exec(
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
      ],
      $ignore,
      $ret
    );
    return $ret;
  }
}

/**
 * Generate names for a specific instance.
 *
 * @param $instance
 *
 * @return string
 * @throws \Exception
 */
function hardwoods_jbrowse_cmd_generate_names($instance) {
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
  hardwoods_jbrowse_safe_exec(
    'perl',
    [
      $bin . '/generate-names.pl',
      '--out',
      $out,
    ],
    $ignore,
    $ret
  );

  return $ret;
}

/**
 * Delete a track to an instance.
 *
 * @param $track
 *
 * @return string
 * @throws \Exception
 */
function hardwoods_jbrowse_cmd_delete_track($track) {
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
      $bin . '/remove-track.pl',
      '--trackLabel',
      hardwoods_jbrowse_make_slug($track->label),
      '--dir',
      $out,
      '--delete',
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
