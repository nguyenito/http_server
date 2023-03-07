import http from 'k6/http';
import { check, sleep } from 'k6';
// Test configuration
export const options = {
  thresholds: {
    // Assert that 99% of requests finish within 3000ms.
    http_req_duration: ['p(99) < 3000'],
  },
  // Ramp the number of virtual users up and down
  stages: [
    { duration: '1s', target: 100 }, //bellow normal load
    { duration: '5s', target: 100 },
    { duration: '1s', target: 200 }, //normal load
    { duration: '5s', target: 200 },
    { duration: '1s', target: 300 }, // around breaking point
    { duration: '5s', target: 300 },
    { duration: '1s', target: 1000 }, // beyond breaking point
    { duration: '5s', target: 1000 },
    { duration: '10s', target: 0 }, //Scale down, recovery stage 0.01ms
  ],
};

// Simulated user behavior
export default function () {
  let res = http.get('http://localhost:8080/1');
  // Validate response status
  check(res, { 'status was 200': (r) => r.status == 200 });
  sleep(1);
}
