import { Declaration, Reference } from "./CodeComponents";

export default function ProjectViewer(props) {
	const data = props.data;

	const is_empty = (x) => { return x == undefined || x.length == 0 }

	return (
		<section>
			<h1 id="Declaration">
				{`${data.project}`}
			</h1>
			<p id="Description">
				{data.detailedDescription}
			</p>
			<h2>Header-files:</h2>
			<ul>
				{data.headers.map((x, i) => {
					return (
						<li key={i}>
							<Reference path={`${data.project}/${x.name}`}>{x.name}</Reference>
						</li>
					)
				})}
			</ul>
		</section>
	)
}