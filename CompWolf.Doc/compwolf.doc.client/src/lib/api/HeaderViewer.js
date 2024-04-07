import { Declaration, Reference } from "./CodeComponents";

export default function HeaderViewer(props) {
	const data = props.data;

	const is_empty = (x) => { return x == undefined || x.length == 0 }

	return (
		<section>
			<small>
				<p id="Project">In project <Reference path={data.project}>{data.project}</Reference></p>
			</small>
			<h1 id="Declaration">
				<Declaration>{`<${data.header}>`}</Declaration>
			</h1>
			<p id="Description">
				{data.detailedDescription}
			</p>
			<h2>Entities:</h2>
			<ul>
				{data.entities.map((x, i) => {
					return (
						<li key={i}>
							<Reference path={`${data.project}/${data.header}/${x.name}`}>{x.name}</Reference>
						</li>
					)
				})}
			</ul>
		</section>
	)
}