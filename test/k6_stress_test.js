import http from 'k6/http';
import { check } from 'k6';

export const options = {
  scenarios: {
    constant_request_rate: {
      executor: 'constant-arrival-rate',
      rate: 100,
      timeUnit: '1s', // 1000 iterations per second, i.e. 1000 RPS
      duration: '30s',
      preAllocatedVUs: 100, // how large the initial pool of VUs would be
      maxVUs: 400, // if the preAllocatedVUs are not enough, we can initialize more
    },
  },
};

export default function () {
  let responses = http.get('http://localhost:8080/3');
  check(responses, {
    'status was 200': (r) => r.status == 200,
  });
}
