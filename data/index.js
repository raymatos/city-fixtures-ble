$('#relays-table').on('click', 'tbody tr', function(event) {
  $(this).addClass('highlight').siblings().removeClass('highlight');
});