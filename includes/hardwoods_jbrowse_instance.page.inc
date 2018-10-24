<?php

function hardwoods_jbrowse_instance_page($instance_id) {
  $instance = hardwoods_jbrowse_get_instance($instance_id);

  if (empty($instance)) {
    drupal_not_found();
    return;
  }

  drupal_set_title("Manage $instance->title JBrowse");

  $content = [];

  $content['instance_table'] = [
    '#type' => 'markup',
    '#markup' => theme(
      'table',
      [
        'header' => ['Key', 'Value'],
        'rows' => [
          ['Instance Name', $instance->title],
          ['Created At', date('m/d/Y', $instance->created_at)],
          [
            'Organism',
            "{$instance->organism->genus} {$instance->organism->species}",
          ],
          ['Created By', $instance->user->name],
        ],
      ]
    ),
  ];

  $tracks = hardwoods_jbrowse_get_tracks($instance);

  if (empty($tracks)) {
    $content['no_tracks'] = [
      '#type' => 'item',
      '#markup' => 'No tracks found for this instance. Please use the add tracks link above to add new tracks.',
    ];
  }
  else {
    $content['tracks_label'] = [
      '#type' => 'item',
      '#markup' => '<h4>Tracks</h4>',
    ];
    $content['tracks_table'] = [
      '#type' => 'markup',
      '#markup' => theme(
        'table',
        [
          'header' => [
            'Label',
            'Track Type',
            'File Type',
            'Created By',
            'Created At',
            'Delete Track'

          ],
          'rows' => array_map(
            function ($track) {
              return [
                $track->label,
                $track->track_type,
                $track->file_type,
                $track->user->name,
                date('m/d/Y', $track->created_at),
                'anything',
              ];
            },
            $tracks
          ),
        ]
      ),
    ];
  }

  return $content;
}
